// Copyright 2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef THIRD_PARTY_NEARBY_SHARING_NEARBY_SHARING_DECODER_IMPL_H_
#define THIRD_PARTY_NEARBY_SHARING_NEARBY_SHARING_DECODER_IMPL_H_

#include <stdint.h>

#include <memory>

#include "dll_config.h"
#include "advertisement.h"
#include "sharing/proto/wire_format.pb.h"

namespace nearby {
    namespace sharing {
        extern "C" {
            class DLL_API NearbySharingDecoderImpl{
            public:
                std::unique_ptr<Advertisement> DecodeAdvertisement(
                    std::vector<uint8_t> data);
                std::unique_ptr<nearby::sharing::service::proto::Frame> DecodeFrame(
                    std::vector<uint8_t> data);
            };

        }  // namespace sharing
    }  // namespace nearby
}

#endif  // THIRD_PARTY_NEARBY_SHARING_NEARBY_SHARING_DECODER_IMPL_H_
