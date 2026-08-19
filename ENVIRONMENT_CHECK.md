# PHASE 0 - ENVIRONMENT CHECK

Thoi diem kiem tra: 2026-08-13  
Pham vi: chi doc moi truong va tao thu muc project moi. Khong sua project cu, Qt, toolchain, Mosquitto hay ACL.

## 1. Host Ubuntu x86_64

| Hang muc | Ket qua | Trang thai |
|---|---|---|
| Working directory | `/home/pi` | PASS |
| User | `pi` (home `/home/pi`) | PASS |
| Architecture | `x86_64` | PASS |
| Kernel | Linux 6.8.0-136-generic | PASS |
| Project moi | `/home/pi/Duy/Hethonggiamsat` | CREATED |
| CMake | 4.4.20260729-g8a947d7 | PASS |

### Toolchain thuc te tren Host

Nhung duong dan trong de bai duoi `/home/admin1/Qt6Cross_Bookworm_651` va
`/opt/cross-pi-gcc-bookworm` khong ton tai tren Host hien tai. Bo cong cu tuong
duong da duoc tim thay tai cac duong dan sau; khong co file nao trong cac thu muc
nay bi thay doi:

| Thanh phan | Duong dan thuc te | Ket qua |
|---|---|---|
| Cross GCC | `/opt/cross-pi-gcc/bin/aarch64-linux-gnu-gcc` | GCC 12.2.0 |
| Cross G++ | `/opt/cross-pi-gcc/bin/aarch64-linux-gnu-g++` | G++ 12.2.0 |
| Sysroot | `/home/pi/Qt6Cross/rpi-sysroot` | PRESENT |
| Qt Host | `/home/pi/Qt6Cross/qt6/host` | Qt 6.5.1 |
| Qt ARM64 staging | `/home/pi/Qt6Cross/qt6/pi` | Qt 6.5.1 |
| CMake toolchain | `/home/pi/Qt6Cross/qt6/pi-build/toolchain.cmake` | PRESENT |

Qt ARM64 staging co cac module bat buoc `Core`, `Gui`, `Widgets`, `Sql`,
`Network` va co them `Mqtt`. Khong tim thay `Qt6Charts`; chart can duoc trien
khai bang Qt Widgets/QPainter de khong rebuild toan bo Qt.

Sysroot co `mosquitto.h`, `libmosquitto.so`, `sqlite3.h` va `libsqlite3.so` cho
aarch64.

## 2. Raspberry Pi 4

SSH: `pi@192.168.137.227` ket noi thanh cong. Mat khau khong duoc ghi vao file
nay hay source code.

| Hang muc | Ket qua | Trang thai |
|---|---|---|
| Hostname | `raspberrypi` | PASS |
| User/home | `pi` / `/home/pi` | PASS |
| Architecture | `aarch64` | PASS |
| OS | Debian GNU/Linux 12 (bookworm) | PASS |
| Kernel | Linux 6.12.96+rpt-rpi-v8 | PASS |
| glibc | 2.36 | PASS |
| Project deploy moi | `/home/pi/Duy/Hethonggiamsat` | CREATED |
| Qt runtime | `/usr/local/qt6`, Qt 6.5.1 | PASS |
| Qt Core/Gui/Widgets/Sql/Network | Installed | PASS |
| Qt SQLite driver | `libqsqlite.so` | PASS |
| QtMqtt runtime | Khong co trong `/usr/local/qt6` | WARNING |
| QtCharts runtime | Khong co trong `/usr/local/qt6` | WARNING |
| SQLite CLI/library/dev | 3.40.1 | PASS |
| Mosquitto broker | 2.0.11, enabled va active | PASS |
| libmosquitto runtime/dev/clients | 2.0.11 | PASS |
| TCP listener | `0.0.0.0:1883` va `[::]:1883` | PASS |

## 3. Mosquitto hien tai

Broker dang dung cau hinh tong `/etc/mosquitto/mosquitto.conf`, include file
`/etc/mosquitto/conf.d/esp32.conf`:

- `listener 1883`
- `allow_anonymous false`
- password file `/etc/mosquitto/passwd`
- ACL file `/etc/mosquitto/acl`

ACL dang phuc vu nhieu project cu (access control, tram thoi tiet, he thong ra
vao). Namespace `air/...` chua duoc cap quyen. PHASE 0 khong thay doi cac file
nay. Neu bo sung o phase MQTT, phai backup va chi them quyen/topic rieng, khong
ghi de cau hinh cu.

## 4. Lua chon ky thuat sau audit

- Build ARM64 se dung cac duong dan thuc te `/home/pi/Qt6Cross` va
  `/opt/cross-pi-gcc`, sau khi co mot smoke test xac nhan binary aarch64.
- MQTT co the dung `libmosquitto` da co tren ca sysroot va Pi; cach nay tranh
  phu thuoc QtMqtt runtime dang thieu tren Pi.
- Bieu do se dung custom Qt Widgets/QPainter vi QtCharts khong co san.
- SQLite dung Qt SQL voi driver `QSQLITE` da xac nhan tren Pi.
- Tai thoi diem audit, SSH shell khong co `DISPLAY` va dung session `tty`; can
  xac dinh graphical session truoc khi chay GUI thu cong.

## 5. Ket luan PHASE 0

PHASE 0 PASS voi mot sai khac duong dan Host so voi de bai. Khong co blocker ve
compiler, Qt co ban, SQLite hay MQTT C library. Chua build, deploy binary, sua
Mosquitto, sua ACL, tao database hoac chay GUI trong phase nay.
