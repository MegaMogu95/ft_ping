from scapy.all import *
import os
# id must equal getpid() & 0xFFFF of YOUR ft_ping process
pid = int(input("ft_ping pid & 0xffff (decimal): "))
orig = IP(src="127.0.0.1", dst="8.8.8.8")/ICMP(type=8, id=pid, seq=42)
def err(t, c, **kw):
    send(IP(src="10.171.48.2", dst="127.0.0.1")/ICMP(type=t, code=c, **kw)/orig)
err(3, 0)   # Destination Net Unreachable
err(3, 1)   # Destination Host Unreachable
err(3, 3)   # Destination Port Unreachable
err(3, 4, nexthopmtu=1400)   # Frag needed and DF set (mtu = 1400)
err(3, 13)  # Communication/Packet filtered
err(5, 1, gw="10.171.48.9")  # Redirect Host (New nexthop: 10.171.48.9)
err(11, 0)  # Time to live exceeded
err(11, 1)  # Frag reassembly time exceeded