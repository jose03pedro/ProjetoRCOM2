/ip address add address=172.16.1.69/24 interface=ether1;
/ip address add address=172.16.61.254/24 interface=ether2;
/ip route add dst-address=172.16.60.0/24 gateway=172.16.61.253;
/ip route add dst-address=0.0.0.0/0 gateway=172.16.61.254;
/ip route add dst-address=0.0.0.0/0 gateway=172.16.1.254;

