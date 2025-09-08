# Commands to Configure Docker in UbuntuVM:
```
sudo apt update
sudo apt upgrade -y
sudo apt install -y ca-certificates curl gnupg lsb-release
sudo mkdir -p /etc/apt/keyrings

curl -fsSL https://download.docker.com/linux/ubuntu/gpg | 
sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg

echo \
  "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] \
  https://download.docker.com/linux/ubuntu \
  $(lsb_release -cs) stable" | \
  sudo tee /etc/apt/sources.list.d/docker.list > /dev/null

sudo apt update
sudo apt install -y docker-ce docker-ce-cli containerd.io\ 
docker-buildx-plugin docker-compose-plugin
docker --version
```

# Commands to configure Ubntu20 conatainer with amd64 platform:
```
docker run --rm --privileged tonistiigi/binfmt --install all
docker pull --platform=linux/amd64 ubuntu:20.04
docker run --platform=linux/amd64 -it ubuntu:20.04 bash
```

# Commands to configure ns3-dce:
```
apt install -y g++ cmake ninja-build ccache libgsl-dev libgtk-3-dev libboost-dev wget git python3 python3-pip
apt install -y automake bc bison flex gawk libc6 libc6-dbg libdb-dev libssl-dev libpcap-dev vim rsync gdb
apt install -y mercurial indent libsysfs-dev
pip3 install requests distro
git clone https://gitlab.com/nsnam/bake.git
cd bake
git checkout -b dce-1.12 origin/dce-1.12
export PATH=$PATH:`pwd`/build/bin
export LD_LIBRARY_PATH=`pwd`/build/lib
export DCE_PATH=`pwd`/build/bin_dce:`pwd`/build/sbin
./bake.py configure -e dce-linux-1.12 -e dce-quagga-1.12
./bake.py show
./bake.py download
./bake.py build
```
