// Copyright 2020 Google LLC
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

#include "internal/platform/implementation/windows/log_message.h"

#include <algorithm>

namespace nearby {
namespace windows {

api::LogMessage::Severity min_log_severity_ = api::LogMessage::Severity::kInfo;


LogMessage::LogMessage(const char* file, int line, Severity severity)
    : log_streamer_(file, line, line) {}

LogMessage::~LogMessage() = default;

void LogMessage::Print(const char* format, ...) {
}

std::ostream& LogMessage::Stream() { return log_streamer_.stream(); }

}  // namespace windows

namespace api {

void LogMessage::SetMinLogSeverity(Severity severity) {
  windows::min_log_severity_ = severity;
}

bool LogMessage::ShouldCreateLogMessage(Severity severity) {
  return severity >= windows::min_log_severity_;
}
}  // namespace api
}  // namespace nearby
