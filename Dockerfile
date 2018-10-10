FROM node:10-alpine
RUN apk update && apk add python make g++

WORKDIR /app

# Global installation of Reason
RUN npm install -g --unsafe-perm reason-cli@3.2.0-linux serve

COPY package*json ./
COPY bsconfig.json ./bsconfig.json
COPY ./src ./src

RUN npm ci -s
RUN npm run build

CMD npm run server
