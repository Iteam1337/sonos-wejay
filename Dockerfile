# Build app
FROM node:11 as builder

WORKDIR /build

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
