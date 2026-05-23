#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <string>
#include <limits>

using namespace std;

// ==============================
// 資料結構
// ==============================
struct Point {
    int x;
    int y;
};

// ==============================
// 工具函式
// ==============================
void printPoints(const vector<Point>& pts, const string& title) {
    cout << title << "\n";

    for (const auto& p : pts) {
        cout << "(" << p.x << "," << p.y << ") ";
    }

    cout << "\n\n";
}

double distancePoints(const Point& a, const Point& b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;

    return sqrt(dx * dx + dy * dy);
}

bool cmpX(const Point& a, const Point& b) {
    if (a.x != b.x)
        return a.x < b.x;

    return a.y < b.y;
}

bool cmpY(const Point& a, const Point& b) {
    if (a.y != b.y)
        return a.y < b.y;

    return a.x < b.x;
}

// =====================================================
// Part A. 2D_Maximal Template
// =====================================================

// 判斷 p1 是否被 p2 凌駕
// 若 p2.x >= p1.x 且 p2.y >= p1.y，並且至少一項嚴格大於，則 p1 被 p2 凌駕
bool isDominated(const Point& p1, const Point& p2) {
    return (p2.x >= p1.x &&
            p2.y >= p1.y &&
            (p2.x > p1.x || p2.y > p1.y));
}

// n <= 3 時直接暴力求極點
vector<Point> bruteForceMaximal(const vector<Point>& S) {
    vector<Point> result;

    // 對每個點檢查是否被其他點凌駕
    for (int i = 0; i < S.size(); i++) {

        bool dominated = false;

        for (int j = 0; j < S.size(); j++) {

            if (i != j && isDominated(S[i], S[j])) {
                dominated = true;
                break;
            }
        }

        // 若未被凌駕，加入結果
        if (!dominated) {
            result.push_back(S[i]);
        }
    }

    return result;
}

// 取得 x 座標中位數
int getMedianX(const vector<Point>& S) {

    vector<int> xs;

    for (const auto& p : S) {
        xs.push_back(p.x);
    }

    // 排序
    sort(xs.begin(), xs.end());

    // 回傳中位數
    return xs[xs.size() / 2];
}

// 2D_Maximal 主遞迴
vector<Point> maximalPoints(const vector<Point>& S) {

    int n = S.size();

    if (n <= 3) {
        return bruteForceMaximal(S);
    }

    // Step 1: 找 x 座標中位數
    int medianX = getMedianX(S);

    // Step 2: 分割成 S_L 與 S_R
    vector<Point> SL, SR;

    // 將 x <= medianX 的點放入 SL
    // 將 x >  medianX 的點放入 SR
    for (const auto& p : S) {

        if (p.x <= medianX)
            SL.push_back(p);
        else
            SR.push_back(p);
    }

    // 避免分割失敗造成無限遞迴
    if (SL.size() == S.size() || SR.size() == S.size()) {
        return bruteForceMaximal(S);
    }

    // Step 3: 遞迴求左右兩側極點
    vector<Point> leftMaximal = maximalPoints(SL);
    vector<Point> rightMaximal = maximalPoints(SR);

    // Step 4: 找出 S_R 極點中 y 最大值
    int ymax = -1;

    // 從 rightMaximal 中找最大的 y
    if (!rightMaximal.empty()) {

        ymax = rightMaximal[0].y;

        for (const auto& p : rightMaximal) {
            ymax = max(ymax, p.y);
        }
    }

    // Step 5: 刪除 S_L 中 y < ymax 的點
    vector<Point> filteredLeft;

    // 若右邊為空，全部保留
    if (rightMaximal.empty()) {
        filteredLeft = leftMaximal;
    }
    else {

        // 只保留 y >= ymax 的點
        for (const auto& p : leftMaximal) {

            if (p.y >= ymax) {
                filteredLeft.push_back(p);
            }
        }
    }

    // Step 6: 合併結果
    vector<Point> result = filteredLeft;

    for (const auto& p : rightMaximal) {
        result.push_back(p);
    }

    return result;
}

// =====================================================
// Part B. 2D_Closest_Pair Template
// =====================================================

// n <= 3 時直接暴力求最近距離
double bruteForceClosest(const vector<Point>& S) {

    int n = S.size();

    // 少於兩點
    if (n < 2) {
        return numeric_limits<double>::infinity();
    }

    double ans = numeric_limits<double>::infinity();

    // 兩兩比較
    for (int i = 0; i < n; i++) {

        for (int j = i + 1; j < n; j++) {

            ans = min(ans,
                      distancePoints(S[i], S[j]));
        }
    }

    return ans;
}

// 2D Closest Pair 遞迴函式
double closestPairRecursive(vector<Point> Px,
                            vector<Point> Py) {

    int n = Px.size();

    // Base case
    if (n <= 3) {
        return bruteForceClosest(Px);
    }

    // Step 1: 取 x 中位數
    int mid = n / 2;

    Point midPoint = Px[mid];

    int L = midPoint.x;

    // Step 2: 分割成左半與右半
    vector<Point> PxL(Px.begin(), Px.begin() + mid);
    vector<Point> PxR(Px.begin() + mid, Px.end());

    vector<Point> PyL, PyR;

    // 依照規則分配 Py
    for (const auto& p : Py) {

        if (p.x < L ||
           (p.x == L && PyL.size() < PxL.size())) {

            PyL.push_back(p);
        }
        else {
            PyR.push_back(p);
        }
    }

    // 避免分割失敗造成無限遞迴
    if (PxL.empty() || PxR.empty()) {
        return bruteForceClosest(Px);
    }

    // Step 3: 遞迴求左右最近距離
    double dL = closestPairRecursive(PxL, PyL);
    double dR = closestPairRecursive(PxR, PyR);

    double delta = min(dL, dR);

    // Step 4: 建立 strip
    vector<Point> strip;

    // 將 |x - L| < delta 的點加入 strip
    for (const auto& p : Py) {

        if (abs(p.x - L) < delta) {
            strip.push_back(p);
        }
    }

    // Step 5: 檢查 strip 中跨中線的最近點
    int m = strip.size();

    for (int i = 0; i < m; i++) {

        // 最多檢查後面有限個點
        for (int j = i + 1;
             j < m &&
             (strip[j].y - strip[i].y) < delta;
             j++) {

            delta = min(delta,
                        distancePoints(strip[i],
                                       strip[j]));
        }
    }

    return delta;
}

// 封裝主函式：先依 x 與 y 排序，再呼叫遞迴
double closestPair(vector<Point> S) {

    // 建立 Px 與 Py
    vector<Point> Px = S;
    vector<Point> Py = S;

    // Px 依 x 排序
    sort(Px.begin(), Px.end(), cmpX);

    // Py 依 y 排序
    sort(Py.begin(), Py.end(), cmpY);

    // 呼叫遞迴
    return closestPairRecursive(Px, Py);
}

// =====================================================
// 主程式
// =====================================================
int main() {

    // 測資 1
    vector<Point> case1 = {
        {5,12}, {12,10}, {2,9}, {5,7}, {10,6},
        {9,4}, {6,3}, {11,4}, {1,3}, {6,1}
    };

    // 測資 2
    vector<Point> case2 = {
        {6,0}, {11,3}, {10,9}, {8,5}, {9,7}, {6,10},
        {4,7}, {5,4}, {3,8}, {2,3}, {2,9}, {9,4}
    };

    // ------------------------------
    // Part A: 2D_Maximal
    // ------------------------------
    printPoints(case1, "Input Case 1:");

    vector<Point> ans1 = maximalPoints(case1);

    sort(ans1.begin(), ans1.end(), cmpX);

    printPoints(ans1,
                "Maximal Points of Case 1:");

    printPoints(case2, "Input Case 2:");

    vector<Point> ans2 = maximalPoints(case2);

    sort(ans2.begin(), ans2.end(), cmpX);

    printPoints(ans2,
                "Maximal Points of Case 2:");

    // ------------------------------
    // Part B: 2D_Closest_Pair
    // ------------------------------
    cout << "Closest Pair Distance of Case 1: "
         << closestPair(case1)
         << "\n";

    cout << "Closest Pair Distance of Case 2: "
         << closestPair(case2)
         << "\n";

    return 0;
}
