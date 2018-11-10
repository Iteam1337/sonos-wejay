FROM node:11-alpine

# Build tools and Git
RUN apk update && apk add python make g++
RUN apk update && apk upgrade && \
  apk add --no-cache bash git openssh coreutils

WORKDIR /app

# Global installation of Reason
RUN npm install -g --unsafe-perm reason-cli@3.2.0-linux serve

COPY package*json ./
COPY bsconfig.json ./bsconfig.json
COPY ./src ./src
COPY ./__tests__ ./__tests__

RUN npm ci
RUN npm run build

CMD npm run server
