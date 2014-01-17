#!/bin/sh

cd `dirname $0`
cd support/o3

if [ -f build/default/o3.node ]; then
    cp build/default/o3.node ../../lib/libxml
else
    cp build/Release/o3.node ../../lib/libxml
fi