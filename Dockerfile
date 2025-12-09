# --- Build Stage ---
FROM ubuntu:22.04 AS builder
WORKDIR /app

# Tools + Conan 2.x + GTest and GMock (for CoroKafka mocks)
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential cmake git ca-certificates curl python3 python3-pip \
    libssl-dev zlib1g-dev wget xz-utils libgtest-dev libgmock-dev \
    && rm -rf /var/lib/apt/lists/* \
    && pip install "conan>=2.0"

# Conan 2.x setup
RUN conan profile detect --force
RUN conan remote add center2 https://center2.conan.io || true

# -------------------------------------------------
# 1. Boost 1.73.0 (last version with forced_unwind)
# -------------------------------------------------
RUN wget -q https://archives.boost.io/release/1.73.0/source/boost_1_73_0.tar.gz && \
    tar xzf boost_1_73_0.tar.gz && \
    cd boost_1_73_0 && \
    ./bootstrap.sh --with-libraries=context,coroutine,thread,system && \
    ./b2 install --prefix=/usr/local threading=multi link=shared -j$(nproc) && \
    cd .. && rm -rf boost_1_73_0 boost_1_73_0.tar.gz

# -------------------------------------------------
# 2. Bloomberg Quantum
# -------------------------------------------------
RUN git clone --depth 1 https://github.com/bloomberg/quantum.git /tmp/quantum && \
    mkdir -p /tmp/quantum/build && cd /tmp/quantum/build && \
    cmake .. -DCMAKE_BUILD_TYPE=Release -DQUANTUM_ENABLE_TESTS=OFF && \
    make install -j$(nproc) && \
    rm -rf /tmp/quantum

# -------------------------------------------------
# 3. Your project + Conan deps
# -------------------------------------------------
COPY conanfile.txt .
COPY src/ src/
COPY CMakeLists.txt .

RUN conan install . --output-folder=build --build=missing

# -------------------------------------------------
# 4. Bloomberg CoroKafka (now compiles with GTest + GMock)
# -------------------------------------------------
RUN git clone --depth 1 https://github.com/bloomberg/corokafka.git /tmp/corokafka && \
    mkdir -p /tmp/corokafka/build && cd /tmp/corokafka/build && \
    cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_SHARED_LIBS=ON \
        -DCORO_KAFKA_BUILD_EXAMPLES=OFF \
        -DCORO_KAFKA_BUILD_TESTS=OFF \
        -DCORO_KAFKA_ENABLE_TESTING=OFF \
        -DCORO_KAFKA_BUILD_MOCKS=OFF \
        -Dcppkafka_DIR=/app/build \
        -DRdKafka_DIR=/app/build \
        -DCMAKE_PREFIX_PATH="/usr/local;/app/build" && \
    make -j$(nproc) && \
    make install && \
    rm -rf /tmp/corokafka

# -------------------------------------------------
# 5. Build your app
# -------------------------------------------------
WORKDIR /app/build
RUN cmake .. \
        -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_PREFIX_PATH="/usr/local;/app/build" && \
    cmake --build . --config Release

# --- Runtime Stage ---
FROM ubuntu:22.04 AS final
WORKDIR /app

RUN apt-get update && apt-get install -y --no-install-recommends \
    libssl3 zlib1g \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /app/build/TradingMicroservice .

EXPOSE 9080
CMD ["./TradingMicroservice"]