It is common to install Node.js into ~/local/bin and then run it from command
line. These scripts do that:

./node.install.sh
./npm.install.sh

(We will start node later. See main README file.)

MongoDB works well with a similar approach:

mkdir -p ~/local/data
cd ~/local
wget http://fastdl.mongodb.org/linux/mongodb-linux-x86_64-2.5.3.tgz
tar xvf mongodb-linux-x86_64-2.5.3.tgz
cd mongodb-linux-x86_64-2.5.3
bin/mongod --dbpath ../data
