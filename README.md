# SimpleShell_OS
Simple Shell on Linux
1) Hướng dẫn chạy chương trình trên Linux
* Bước 1: Khởi động hệ điều hành Linux
* Bước 2: Mở thư mục chứa file SimpleShell_OS.c
* Bước 3: Mở terminal trên thư mục chứa file SimpleShell.c
* Bước 4: Gõ lệnh sau trên terminal: gcc SimpleShell_OS.c -o SimpleShell 
* Bước 5: Gõ tiếp lệnh trên terminal: ./SimpleShell

2) Hướng dẫn chạy các lệnh cơ bản đã làm trong chương trình
* Executing Command in a Child Process:
  * Trong chương trình SimpleShell gõ lệnh VD: osh>ls -l
  * Nếu muốn tiến trình cha đợi tiến trình con thì thêm dấu & sau cùng câu lênh VD: osh>ls -l &
* Creating a History Feature:
  * Trong chương trình SimpleShell gõ lệnh !! để gọi lại lịch sử, Nếu trong lịch sử không có command nào thì chương trình sẽ trả về “No commands in history.” VD: osh> !!
* Redirecting Input and Output:
  * Trong chương trình SimpleShell osh>ls > out.txt
  * Trong chương trình SimpleShell osh>sort < in.txt
* Communication via a Pipe
  * Trong chương trình SimpleShell osh>ls -l | less

3) Ghi chú:
Nếu shell xuất hiện osh>osh> thì vẫn cứ nhập lệnh và chạy bình thường, đây là lỗi hiện thị và nhóm đang cố gắng khắc phục.

4) Thành viên nhóm:
* 1712500 Trần Phúc Quang Huy
* 1712528 Ngô Trường Khiêm
* 1712555 Cháu Vĩnh Lập
