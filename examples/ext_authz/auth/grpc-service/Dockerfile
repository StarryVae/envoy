FROM golang:1.20.4-bullseye@sha256:8251849ec0e15a94170dec220e2da16b0d066e82dfbb462ac8402818de8050cc as builder

COPY . /app
RUN make -C /app/grpc-service

FROM debian:bullseye-slim

COPY --from=builder /app/grpc-service/server /app/server
CMD ["/app/server", "-users", "/etc/users.json"]
