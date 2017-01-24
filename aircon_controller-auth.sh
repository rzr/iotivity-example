set -x
./aircon_controller 172.17.0.1:5683 github 147b0ca1bfacf80a109e 0   2>&1 | tee "$0.log"
