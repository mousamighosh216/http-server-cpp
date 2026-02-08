# ---------- build stage ----------
FROM gcc:13 AS builder

RUN apt-get update && apt-get install -y cmake make

WORKDIR /http-server-cpp
COPY . .

RUN rm -rf build && mkdir build && cd build \
    && cmake .. \
    && cmake --build .

# ---------- runtime stage ----------
FROM debian:stable-slim

WORKDIR /http-server-cpp

# copy only compiled binary
COPY --from=builder /http-server-cpp/build/web_server .

# copy html folder
COPY html ./html

EXPOSE 6969

CMD ["./web_server"]

