FROM debian:bookworm AS builder
ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y --no-install-recommends \
      ca-certificates curl bash git \
      build-essential g++ pkg-config unzip \
      openjdk-17-jre-headless \
      libantlr4-runtime-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /tmp/task-install
RUN curl -sSL https://taskfile.dev/install.sh \
     | bash -s -- -b /usr/local/bin \
  && rm -rf /tmp/task-install

RUN curl -L -o /usr/local/lib/antlr-4.8-complete.jar \
      https://www.antlr.org/download/antlr-4.8-complete.jar

RUN printf '#!/usr/bin/env bash\n\
exec java -Xmx500M -cp /usr/local/lib/antlr-4.8-complete.jar org.antlr.v4.Tool "$@"\n' \
  > /usr/local/bin/antlr4 \
 && chmod +x /usr/local/bin/antlr4 \
 && ln -sf /usr/local/bin/antlr4 /usr/local/bin/antlr

WORKDIR /app
COPY . .

RUN task build ANTLR_RUNTIME=/usr/include/antlr4-runtime ANTLR_LIB=/usr/lib/x86_64-linux-gnu