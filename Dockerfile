FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    git cmake g++ curl zip unzip tar pkg-config ninja-build \
    && rm -rf /var/lib/apt/lists/*

# vcpkg for the Crow dependency (same as the local Windows build).
RUN git clone --depth 1 https://github.com/microsoft/vcpkg /opt/vcpkg \
    && /opt/vcpkg/bootstrap-vcpkg.sh -disableMetrics
RUN /opt/vcpkg/vcpkg install crow

WORKDIR /app
COPY . .

RUN cmake -S . -B build -G Ninja \
    -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake \
    -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build

# Render injects $PORT; the server reads it (defaults to 8080).
CMD ["./build/numerix"]
