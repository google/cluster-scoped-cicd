# Copyright 2020 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

FROM gcr.io/asylo-framework/asylo:buildenv-v0.5.2

WORKDIR /workdir

COPY . /workdir

RUN apt-get install curl gnupg -y
RUN curl -f https://bazel.build/bazel-release.pub.gpg | apt-key add -
RUN echo "deb [arch=amd64] https://storage.googleapis.com/bazel-apt stable jdk1.8" | tee /etc/apt/sources.list.d/bazel.list
RUN apt-get update -y
RUN apt-get install bazel -y

RUN which bazel

RUN bazel --output_user_root=./output build --config=sgx-sim :asylo-hello-world

ENTRYPOINT ./bazel-bin/asylo-hello-world
CMD "World!"
