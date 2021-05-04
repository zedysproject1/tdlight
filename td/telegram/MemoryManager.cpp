//
// Copyright Andrea Cavalli (nospam@warp.ovh) 2020
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#include "td/telegram/MemoryManager.h"

#include "td/telegram/secret_api.h"
#include "td/telegram/td_api.h"
#include "td/telegram/telegram_api.h"

#include "td/telegram/AccessRights.h"
#include "td/telegram/AuthManager.h"
#include "td/telegram/ConfigManager.h"
#include "td/telegram/ConfigShared.h"
#include "td/telegram/ContactsManager.h"
#include "td/telegram/DialogId.h"
#include "td/telegram/Document.h"
#include "td/telegram/DocumentsManager.h"
#include "td/telegram/FileReferenceManager.h"
#include "td/telegram/files/FileLocation.h"
#include "td/telegram/files/FileManager.h"
#include "td/telegram/WebPagesManager.h"
#include "td/telegram/StickersManager.h"
#include "td/telegram/VideoNotesManager.h"
#include "td/telegram/VideosManager.h"
#include "td/telegram/AudiosManager.h"
#include "td/telegram/AnimationsManager.h"
#include "td/telegram/GroupCallManager.h"
#include "td/telegram/BackgroundManager.h"
#include "td/telegram/PollManager.h"
#include "td/telegram/InlineQueriesManager.h"
#include "td/telegram/files/FileType.h"
#include "td/telegram/Global.h"
#include "td/telegram/LanguagePackManager.h"
#include "td/telegram/logevent/LogEvent.h"
#include "td/telegram/MessagesManager.h"
#include "td/telegram/misc.h"
#include "td/telegram/net/DcId.h"
#include "td/telegram/net/MtprotoHeader.h"
#include "td/telegram/Td.h"
#include "td/telegram/TdDb.h"

#include "td/actor/MultiPromise.h"
#include "td/actor/PromiseFuture.h"
#include "td/actor/SleepActor.h"

#include "td/db/SqliteKeyValue.h"
#include "td/db/SqliteKeyValueAsync.h"

#include "td/utils/format.h"
#include "td/utils/logging.h"
#include "td/utils/misc.h"
#include "td/utils/PathView.h"
#include "td/utils/Random.h"
#include "td/utils/Slice.h"
#include "td/utils/Time.h"
#include "td/utils/tl_helpers.h"
#include "td/utils/utf8.h"

#include <algorithm>
#include <limits>
#include <type_traits>
#include <unordered_set>
#include <numeric>

namespace td {

tl_object_ptr<td_api::memoryStatistics> MemoryStats::get_memory_statistics_object() const {
  return make_tl_object<td_api::memoryStatistics>(debug);
}

MemoryManager::MemoryManager(Td *td, ActorShared<> parent) : td_(td), parent_(std::move(parent)) {
}

void MemoryManager::start_up() {
  LOG(INFO) << "Start MemoryManager";
}

void MemoryManager::tear_down() {
  LOG(INFO) << "Stopping MemoryManager";
  parent_.reset();
  LOG(INFO) << "Stopped MemoryManager";
}

bool MemoryManager::can_manage_memory() const {
  if (!(td_->auth_manager_->is_authorized() && !G()->close_flag())) {
    return false;
  }
  if (!do_session_settings_allow_for_memory_management()) {
    return false;
  }
  return true;
}

bool MemoryManager::do_session_settings_allow_for_memory_management() {
  if (G()->parameters().use_message_db || G()->parameters().use_chat_info_db || G()->parameters().use_file_db) {
    return false;
  }
  return true;
}

void MemoryManager::get_memory_stats(bool full, Promise<MemoryStats> promise) const {
  if (!can_manage_memory()) {
    auto value = MemoryStats("{}");

    promise.set_value(std::move(value));
    return;
  }

  vector<string> output = {"{\"memory_stats\":{"};

  output.push_back("\"messages_manager_\":{");
  td_->messages_manager_->memory_stats(output);
  output.push_back("}");

  output.push_back(",");

  output.push_back("\"contacts_manager_\":{");
  td_->contacts_manager_->memory_stats(output);
  output.push_back("}");

  output.push_back(",");

  output.push_back("\"web_pages_manager_\":{");
  td_->web_pages_manager_->memory_stats(output);
  output.push_back("}");

  output.push_back(",");

  output.push_back("\"stickers_manager_\":{");
  td_->stickers_manager_->memory_stats(output);
  output.push_back("}");

  output.push_back(",");

  output.push_back("\"documents_manager_\":{");
  td_->documents_manager_->memory_stats(output);
  output.push_back("}");

  output.push_back(",");

  output.push_back("\"video_notes_manager_\":{");
  td_->video_notes_manager_->memory_stats(output);
  output.push_back("}");

  output.push_back(",");

  output.push_back("\"videos_manager_\":{");
  td_->videos_manager_->memory_stats(output);
  output.push_back("}");

  output.push_back(",");

  output.push_back("\"audios_manager_\":{");
  td_->audios_manager_->memory_stats(output);
  output.push_back("}");

  output.push_back(",");

  output.push_back("\"animations_manager_\":{");
  td_->animations_manager_->memory_stats(output);
  output.push_back("}");

  output.push_back(",");

  output.push_back("\"file_manager_\":{");
  td_->file_manager_->memory_stats(output);
  output.push_back("}");

  output.push_back(",");

  output.push_back("\"file_reference_manager_\":{");
  td_->file_reference_manager_->memory_stats(output);
  output.push_back("}");

  output.push_back(",");

  output.push_back("\"group_call_manager_\":{");
  td_->group_call_manager_->memory_stats(output);
  output.push_back("}");

  output.push_back(",");

  output.push_back("\"background_manager_\":{");
  td_->background_manager_->memory_stats(output);
  output.push_back("}");

  output.push_back(",");

  output.push_back("\"inline_queries_manager_\":{");
  td_->inline_queries_manager_->memory_stats(output);
  output.push_back("}");


  output.push_back(",");

  output.push_back("\"poll_manager_\":{");
  td_->poll_manager_->memory_stats(output);
  output.push_back("}");

  output.push_back("}}");

  string s;
  s = accumulate(output.begin(), output.end(), s);
  auto value = MemoryStats(s);

  promise.set_value(std::move(value));
}

void MemoryManager::clean_memory(bool full, Promise<Unit> promise) const {
  if (!can_manage_memory()) {
    if (!do_session_settings_allow_for_memory_management()) {
      promise.set_error(Status::Error(405, "MEMORY_STATS_DISALLOWED"
        " Session settings don't allow memory optimization."
        " If you want to optimize memory, you should completely disable all databases."));
    } else {
      promise.set_error(Status::Error(405, "Can't manage memory now"));
    }
    return;
  }

  td_->messages_manager_->memory_cleanup();
  td_->contacts_manager_->memory_cleanup();
  td_->web_pages_manager_->memory_cleanup();
  td_->stickers_manager_->memory_cleanup();
  td_->documents_manager_->memory_cleanup();
  td_->video_notes_manager_->memory_cleanup();
  td_->videos_manager_->memory_cleanup();
  td_->audios_manager_->memory_cleanup();
  td_->animations_manager_->memory_cleanup();
  td_->file_manager_->memory_cleanup();
  td_->file_reference_manager_->memory_cleanup();
  td_->group_call_manager_->memory_cleanup();
  td_->background_manager_->memory_cleanup();
  td_->inline_queries_manager_->memory_cleanup();
  td_->poll_manager_->memory_cleanup();

  #ifdef __linux__
    #if defined(__GLIBC__) && !defined(__UCLIBC__) && !defined(__MUSL__)
      malloc_trim(0);
    #endif
  #endif

  promise.set_value(Unit());
}

void MemoryManager::get_current_state(vector<td_api::object_ptr<td_api::Update>> &updates) const {
  if (td_->auth_manager_->is_bot()) {
    return;
  }

  // Never return updates
}

}  // namespace td
