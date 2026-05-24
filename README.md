# TarSau - Simple C Archiver

A simple command-line utility written in C to archive and extract ASCII text files on Linux.

## 🛠️ Compilation
bash
make clean && make

## 💻 Usage

### 1. Archive text files (-b)
bash
./bin/tarsau -b t1.txt t2.txt -o myarchive.sau

### 2. Extract archive (-a)
bash
./bin/tarsau -a myarchive.sau output

```

📋 Constraints
Maximum 32 input files.
Combined file size must not exceed 200 MB.
Supports ASCII text files only.

### This project was made by 
G231210554 - Ammar AJAM
and
G231210563 - Abdullah ALLABBAN