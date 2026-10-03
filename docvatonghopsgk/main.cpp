#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <set>
#include <map>
#include <cmath>
#include <algorithm>
#include <fstream>

using namespace std;

// Cấu trúc dữ liệu lưu bài học SGK cũ đọc từ file
struct OldLesson {
    string filepath;
    string content;
};

// Cấu trúc dữ liệu khung SGK mới cần biên soạn
struct NewTopic {
    int id;
    string title;
    string required_concept;
};

// Hàm đọc toàn bộ nội dung từ 1 file .txt
string readFileContent(const string& filepath) {
    ifstream file(filepath);
    if (!file.is_open()) {
        cerr << "[Lỗi] Không thể mở file: " << filepath << endl;
        return "";
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Hàm tách từ, làm sạch dấu câu & chuyển về chữ thường (Tokenization)
vector<string> tokenize(const string& text) {
    vector<string> tokens;
    stringstream ss(text);
    string word;
    while (ss >> word) {
        // Loại bỏ dấu câu
        word.erase(remove_if(word.begin(), word.end(), ::ispunct), word.end());
        // Chuyển thành chữ thường
        for (auto &c : word) c = tolower(c);
        if (word.length() > 1) {
            tokens.push_back(word);
        }
    }
    return tokens;
}

// Hàm tính toán độ tương đồng Cosine Similarity (%) giữa 2 đoạn văn bản
double calculateCosineSimilarity(const string& text1, const string& text2) {
    auto tokens1 = tokenize(text1);
    auto tokens2 = tokenize(text2);

    map<string, int> freq1, freq2;
    set<string> allWords;

    for (const auto& w : tokens1) { freq1[w]++; allWords.insert(w); }
    for (const auto& w : tokens2) { freq2[w]++; allWords.insert(w); }

    double dotProduct = 0.0, normA = 0.0, normB = 0.0;

    for (const auto& word : allWords) {
        int count1 = freq1[word];
        int count2 = freq2[word];
        dotProduct += count1 * count2;
        normA += count1 * count1;
        normB += count2 * count2;
    }

    if (normA == 0.0 || normB == 0.0) return 0.0;
    return (dotProduct / (sqrt(normA) * sqrt(normB))) * 100.0;
}

int main() {
    cout << "========================================================\n";
    cout << "   AI SMART TEXTBOOK - C++ KNOWLEDGE MATCHING ENGINE    \n";
    cout << "========================================================\n\n";

    // 1. Danh sách các file SGK cũ cần nạp vào hệ thống
    vector<string> inputFiles = {
        "data_test/sgk_cu_sinh11.txt",
        "data_test/sgk_cu_vatly12.txt"
    };

    vector<OldLesson> oldLessons;
    for (const auto& path : inputFiles) {
        string text = readFileContent(path);
        if (!text.empty()) {
            oldLessons.push_back({path, text});
            cout << "[Nạp thành công]: " << path << endl;
        }
    }

    if (oldLessons.empty()) {
        cerr << "\n[Cảnh báo] Không tìm thấy dữ liệu trong data_test/! Hãy kiểm tra lại file input.\n";
        return 1;
    }

    // 2. Định nghĩa khung chương trình SGK Mới cần biên soạn
    vector<NewTopic> newTopics = {
        {1, "SGK Mới: Sinh học - Bài Quang hợp ở thực vật", "Quang hợp ở thực vật diệp lục hấp thụ ánh sáng giải phóng O2 tổng hợp chất hữu cơ CO2 H2O"},
        {2, "SGK Mới: Vật lý - Mô tả Dao động cơ học", "Dao động điều hòa phương trình li độ hàm cosin sin biên độ A tần số góc omega"}
    };

    // Mở file xuất kết quả
    ofstream outFile("Nhan_Ban_SGK_Moi.txt");
    if (!outFile.is_open()) {
        cerr << "[Lỗi] Không thể tạo file đầu ra Nhan_Ban_SGK_Moi.txt\n";
        return 1;
    }

    // 3. Tiến hành đối soát ngữ nghĩa & tổng hợp nội dung
    cout << "\n--------------------------------------------------------\n";
    cout << "TIẾN HÀNH PHÂN TÍCH & TRUY VẤN TRI THỨC:\n";

    for (const auto& topic : newTopics) {
        cout << "\n[MỤC TIÊU]: " << topic.title << "\n";
        outFile << "========================================================\n";
        outFile << "[BÀI HỌC TỔNG HỢP]: " << topic.title << "\n";

        double maxScore = -1.0;
        string bestMatchFile = "";
        string bestMatchContent = "";

        for (const auto& lesson : oldLessons) {
            double score = calculateCosineSimilarity(lesson.content, topic.required_concept);
            cout << "  + So sánh với [" << lesson.filepath << "] -> Độ khớp: " << score << "%\n";

            if (score > maxScore) {
                maxScore = score;
                bestMatchFile = lesson.filepath;
                bestMatchContent = lesson.content;
            }
        }

        cout << "=> AI LỰA CHỌN: File " << bestMatchFile << " (Độ tin cậy: " << maxScore << "%)\n";
        outFile << "Nguồn trích xuất chính: " << bestMatchFile << " (Độ khớp: " << maxScore << "%)\n";
        outFile << "Nội dung biên soạn:\n" << bestMatchContent << "\n\n";
    }

    outFile.close();
    cout << "\n========================================================\n";
    cout << "[THÀNH CÔNG] Đã xuất bản thảo ra file 'Nhan_Ban_SGK_Moi.txt'!\n";
    return 0;
}