# ---------- build stage ----------
FROM gcc:13 AS builder

RUN apt-get update && apt-get install -y cmake make

WORKDIR /app
COPY . .

RUN rm -rf build && mkdir build && cd build \
    && cmake .. \
    && cmake --build .

# ---------- runtime stage ----------
FROM debian:stable-slim

WORKDIR /app

# copy only compiled binary
COPY --from=builder /app/build/web_server .

EXPOSE 6969

CMD ["./web_server"]
