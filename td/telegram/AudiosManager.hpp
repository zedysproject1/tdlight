//
// Copyright Aliaksei Levin (levlam@telegram.org), Arseny Smirnov (arseny30@gmail.com) 2014-2021
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "td/telegram/AudiosManager.h"

#include "td/telegram/files/FileId.hpp"
#include "td/telegram/Photo.hpp"
#include "td/telegram/Version.h"

#include "td/utils/common.h"
#include "td/utils/tl_helpers.h"

#include "td/telegram/ConfigShared.h"

namespace td {

template <class StorerT>
void AudiosManager::store_audio(FileId file_id, StorerT &storer) const {
  auto it = audios_.find(file_id);
  CHECK(it != audios_.end());
  const Audio *audio = it->second.get();
  store(audio->file_name, storer);
  store(audio->mime_type, storer);
  store(audio->duration, storer);
  store(audio->title, storer);
  store(audio->performer, storer);
  store(audio->minithumbnail, storer);
  store(audio->thumbnail, storer);
  store(file_id, storer);
}

template <class ParserT>
FileId AudiosManager::parse_audio(ParserT &parser) {
  auto audio = make_unique<Audio>();

  string tmp_filename;
  parse(tmp_filename, parser);

  parse(audio->mime_type, parser);

  if ( G()->shared_config().get_option_boolean("disable_document_filenames") && (
      audio->mime_type.rfind("image/") == 0 ||
      audio->mime_type.rfind("video/") == 0 ||
      audio->mime_type.rfind("audio/") == 0)) {
    audio->file_name = "0";
  } else {
    audio->file_name = tmp_filename;
  }

  parse(audio->duration, parser);
  parse(audio->title, parser);
  parse(audio->performer, parser);
  if (parser.version() >= static_cast<int32>(Version::SupportMinithumbnails)) {
    string tmp_minithumbnail;
    parse(tmp_minithumbnail, parser);
    if (!G()->shared_config().get_option_boolean("disable_minithumbnails")) {
      audio->minithumbnail = tmp_minithumbnail;
    }
  }
  parse(audio->thumbnail, parser);
  parse(audio->file_id, parser);
  if (parser.get_error() != nullptr || !audio->file_id.is_valid()) {
    return FileId();
  }
  return on_get_audio(std::move(audio), false);
}

}  // namespace td
