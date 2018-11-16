tunctl -u $SUDO_USER
echo 1 > /proc/sys/net/ipv4/conf/tap0/proxy_arp
sudo ip link set tap0 up

tunctl -u $SUDO_USER
echo 1 > /proc/sys/net/ipv4/conf/tap1/proxy_arp
sudo ip link set tap1 up

brctl addbr br0
brctl addif br0 tap0
brctl addif br0 tap1
ip link set br0 up
