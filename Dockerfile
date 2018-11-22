# Build app
FROM node:11-alpine as builder

## Build tools and Git
RUN apk update && apk add python make g++
RUN apk update && apk upgrade && \
  apk add --no-cache bash git openssh coreutils

WORKDIR /build

## Global installation of Reason
RUN npm install -g --unsafe-perm reason-cli@3.2.0-linux serve

COPY package*json ./
COPY bsconfig.json ./bsconfig.json
COPY ./src ./src
COPY ./__tests__ ./__tests__

RUN npm ci
RUN npm run build

# App
FROM node:11-alpine

WORKDIR /app

COPY --from=builder /build ./

CMD npm run server
