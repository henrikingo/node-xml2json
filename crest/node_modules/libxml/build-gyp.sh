#!/bin/sh

cd `dirname $0`
cd support/o3

node tools/gluegen.js
node-gyp configure
node-gyp build