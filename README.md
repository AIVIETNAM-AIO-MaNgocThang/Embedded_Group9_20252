# Geometry Dash - STM32F429 Discovery Kit

Dự án mô phỏng tựa game nổi tiếng Geometry Dash chạy trên phần cứng nhúng **STM32F429I-DISCO**, thuộc môn học Hệ thống Nhúng (Embedded Systems) của kỳ 2025.2, Đại học Bách Khoa Hà Nội.
Thành viên:
- Ma Ngọc Thắng - 20235829
- Lãnh Ngọc Khánh - 20235753
- Hoàng Việt - 20235871

Game được phát triển sử dụng ngôn ngữ **C/C++** trên môi trường **STM32CubeIDE** và giao diện đồ họa được thiết kế bằng **TouchGFX**.

## Tính năng hiện tại (MVP)
Dự án hiện tại đã đạt đến mức Minimum Viable Product (MVP) với một màn chơi hoàn chỉnh có thể chơi mượt mà trên kit thực tế. Các tính năng đã được triển khai bao gồm:
- **Hệ thống màn hình cơ bản:** Menu chính, màn hình chơi game (Game Screen), màn hình kết quả (Failed Screen, Clear Screen).
- **Vật lý & Chuyển động:** 
  - Mô phỏng trọng lực, nhảy (nhấn vào màn hình) và rơi tự do cho nhân vật (Cube).
  - Nền và chướng ngại vật cuộn liên tục tạo cảm giác nhân vật tiến về phía trước.
- **Hệ thống chướng ngại vật đa dạng:**
  - **Spike (Gai):** Chạm vào từ bất kỳ hướng nào đều dẫn đến cái chết (Game Over).
  - **Platform (Bục):** Nhân vật có thể đứng trên bục hoặc nhảy lên bục từ dưới lên.
  - **Step (Bậc):** Các khối hộp có thể xếp chồng lên nhau tạo thành địa hình. Nhân vật có thể di chuyển trên bề mặt trên cùng, nhưng va chạm vào mặt bên hoặc mặt dưới sẽ dẫn đến Game Over (hoặc bị chặn).
- **Hệ thống màn chơi tự định nghĩa (.gdl):**
  - Màn chơi được thiết kế thông qua cấu trúc dữ liệu nhị phân tùy chỉnh (đuôi file `.gdl`).
  - Hỗ trợ parser tự động đọc file `.gdl` và giải quyết các trường hợp chướng ngại vật đè lên nhau cùng tọa độ.
- **Thống kê phần chơi:** Ghi nhận và hiển thị phần trăm tiến độ (Progress), số lần thử (Attempts), số bước nhảy (Jumps) và thời gian chơi (Time) trên màn hình Failed/Clear.

## Cách setup
Bước 1: Tải STM32CubeIDE 1.9 trở lên, TouchGFXDesigner 4.24.1 trở lên, và mã nguồn dự án.

Bước 2: Mở file `.project` trong folder con `STM32CubeIDE` của mã nguồn dự án.

Bước 3: Trong STM32CubeIDE, tiến hành Clean Project, Build Project, gắn kit STM32F429 Discovery Kit qua bộ nạp ST-LINK và Run.

## Lộ trình phát triển tiếp theo
Dựa trên MVP hiện tại, dự án có kế hoạch mở rộng các tính năng sau theo thứ tự ưu tiên:
### 1. Hỗ trợ điều khiển nhảy bằng phím USER_BUTTON (PA0) - Ưu tiên: Rất cao
- **Tính khả thi:** Rất cao.
- **Công sức:** Thấp.
- Đọc trạng thái chân PA0 (hoặc dùng ngắt EXTI), gửi tín hiệu nút vào Presenter để cho vật thể nhảy lên.
### 2. Chọn nhiều màn chơi (Level Selection) - Ưu tiên: Cao
- **Tính khả thi:** Rất cao.
- **Công sức:** Thấp - Trung bình.
- Bổ sung UI trên TouchGFX (Sử dụng ScrollList hoặc SwipeContainer).
- Cho phép người chơi xem trước thông tin màn chơi (độ dài, biểu tượng độ khó) và chọn màn chơi yêu thích. Lưu tiến trình vào Model để GameScreen tải màn chơi tương ứng.
### 3. Chế độ Phi thuyền (Ship Mode) - Ưu tiên: Cao
- **Tính khả thi:** Rất cao.
- **Công sức:** Trung bình.
- Thêm chế độ chơi mới bên cạnh chế độ Cube mặc định (sử dụng cờ định dạng trong file .gdl).
- Cơ chế vật lý khác biệt: Giữ màn hình để tăng tốc hướng lên, thả ra để rơi xuống theo trọng lực.
- Địa hình bao gồm cả trần và đất. Va chạm với chướng ngại vật (Step), trần hay đất từ bất kỳ hướng nào đều dẫn đến Game Over.
### 4. Lưu trữ tiến độ (Save Progress) - Ưu tiên: Trung bình
- **Tính khả thi:** Cao.
- **Công sức:** Trung bình.
- Ghi nhận và lưu lại kỷ lục (phần trăm tiến độ cao nhất) hoặc số lần hoàn thành của từng màn chơi.
- Sử dụng bộ nhớ Flash nội (Internal Flash) của STM32 hoặc Backup SRAM (với nguồn pin VBAT) để đảm bảo dữ liệu không bị mất khi tắt nguồn. Cần chú ý ghi Flash nội có thể làm treo CPU ngắn hạn.
### 5. Phát nhạc nền (Background Music) - Ưu tiên: Thấp
- **Tính khả thi:** Cao.
- **Công sức:** Lớn.
- Xuất âm thanh 8-bit qua 1 trong 2 kênh DAC tích hợp của STM32F429 ra loa ngoài.
- Ứng dụng DMA kết hợp Timer để xuất tín hiệu tần số 4KHz đồng bộ, không làm ảnh hưởng đến hiệu năng 60 FPS của TouchGFX. File âm thanh lưu trữ dưới dạng mảng C.
- Yêu cầu cấu hình thêm phần cứng khuếch đại âm thanh (vd: PAM8403, LM386) và loa vật lý.
---
