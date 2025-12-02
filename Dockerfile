# --- Build Stage ---
FROM ubuntu:22.04 AS builder
WORKDIR /app

# Install dependencies and tools
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    curl \
    python3 \
    python3-pip \
    ca-certificates \
    git \
    cmake
RUN pip install conan

# Copy conanfile.txt and source code
COPY conanfile.txt .
COPY src/ src/
COPY CMakeLists.txt .

# Install dependencies and configure the build
RUN conan profile detect --force
RUN conan install . --output-folder=build --build=missing

# Build the project
WORKDIR /app/build
RUN cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release
RUN cmake --build .

# --- Run Stage ---
FROM ubuntu:22.04 AS final
WORKDIR /app

# Copy the built executable from the builder stage
COPY --from=builder /app/build/TradingMicroservice .

# The application runs on port 9080
EXPOSE 9080

# Command to run the microservice when the container starts
CMD ["./TradingMicroservice"]
