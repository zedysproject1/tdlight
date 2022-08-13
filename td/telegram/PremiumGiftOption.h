//
// Copyright Aliaksei Levin (levlam@telegram.org), Arseny Smirnov (arseny30@gmail.com) 2014-2022
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#pragma once

#include "td/telegram/td_api.h"
#include "td/telegram/telegram_api.h"

#include "td/utils/common.h"

namespace td {

class PremiumGiftOption {
  int32 months_ = 0;
  string currency_;
  int64 amount_ = 0;
  string bot_url_;
  string store_product_;

  friend bool operator<(const PremiumGiftOption &lhs, const PremiumGiftOption &rhs);

  friend bool operator==(const PremiumGiftOption &lhs, const PremiumGiftOption &rhs);

  double get_monthly_price() const;

 public:
  PremiumGiftOption() = default;
  explicit PremiumGiftOption(telegram_api::object_ptr<telegram_api::premiumGiftOption> &&option);

  td_api::object_ptr<td_api::premiumGiftOption> get_premium_gift_option_object(
      const PremiumGiftOption &base_option) const;

  template <class StorerT>
  void store(StorerT &storer) const;

  template <class ParserT>
  void parse(ParserT &parser);
};

bool operator==(const PremiumGiftOption &lhs, const PremiumGiftOption &rhs);
bool operator!=(const PremiumGiftOption &lhs, const PremiumGiftOption &rhs);

}  // namespace td
