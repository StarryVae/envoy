FROM golang:1.20.3-bullseye@sha256:0c4028f241827951ee41df718abdb75769b63610f3b0e2350cf6fede68e24d6f as builder

COPY . /app
RUN make -C /app/grpc-service

FROM debian:bullseye-slim

COPY --from=builder /app/grpc-service/server /app/server
CMD ["/app/server", "-users", "/etc/users.json"]
