#!/usr/bin/env sh

set -eo pipefail

rm -rf bootstrap 
docker build . --tag wejay_server -f ./Dockerfile
docker rm server || true
docker create --name server wejay_server 
docker cp server:/app/bootstrap bootstrap 

cd $root_path
