/interface bridge add name=bridge60;
/interface bridge add name=bridge61;
/interface bridge port remove [find interface=ether6];
/interface bridge port remove [find interface=ether10];
/interface bridge port remove [find interface=ether14];
/interface bridge port add bridge=bridge60 interface=ether6;
/interface bridge port add bridge=bridge60 interface=ether10;
/interface bridge port add bridge=bridge61 interface=ether14;
/interface bridge port remove [find interface=ether2];
/interface bridge port add bridge=bridge61 interface=ether2;
/interface bridge port remove [find interface=ether8];
/interface bridge port add bridge=bridge61 interface=ether8;