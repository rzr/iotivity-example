set -x
./aircon_controller 172.17.0.1:5683 github e588d2991b53d63bf8ea 0   2>&1 | tee "$0.log"
