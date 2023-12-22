ifconfig eth0 172.16.60.1/24 up
route add -net 172.16.61.0/24 gw 172.16.60.254
route add default gw 172.16.60.254

