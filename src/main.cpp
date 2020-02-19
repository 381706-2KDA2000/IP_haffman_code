#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include<cmath>
#include<vector>
#include<string>
#include <stack>
#include <bitset>
using namespace cv;

std::vector<int> get_histogramme(Mat src)
{
    std::vector<int> hisogramme(256);
    for (int y = 0; y < src.rows; y++)
        for (int x = 0; x < src.cols; x++)
        {
            Vec3b bgr = src.at<Vec3b>(y, x);
            hisogramme[bgr[0] * 0.11f + bgr[1] * 0.59f + bgr[2] * 0.3f]++;
        }
    return hisogramme;
}

struct HaffmanNode
{
    HaffmanNode* left;
    HaffmanNode* right;
    int val;
    int freq;
};

class HaffmanTree
{
    HaffmanNode* root;
	std::vector<std::string> encodingTable;
public:
    HaffmanTree(Mat);
    void CreateEncodingTable();
	void CreateEncodingTable(HaffmanNode* node, std::string code, std::string direct);
	std::string getSimbolCode(int val);
	std::vector<std::string> getImgEncoded(Mat src);
	Mat CreateImg(std::vector<std::string>);
	int getColor(std::string s);
    ~HaffmanTree();
};

HaffmanTree::HaffmanTree(Mat src)
{
    std::vector<int> hist = get_histogramme(src);
    std::vector<HaffmanNode*> queue(256);
    for (int i = 0; i < 256; i++) {
        queue[i] = new HaffmanNode();
        queue[i]->val = i;
        queue[i]->freq = hist[i];
    }
    for (int j = 0; j < 255; j++)
    {
        int f = 0;
        if (j == 254)
            int a = 0;;
        for (int i = 0; i < 255 - j; i++)
        {
            if (hist[queue[i]->val] < hist[queue[i + 1]->val])
            {
                HaffmanNode* buff = queue[i];
                queue[i] = queue[i + 1];
                queue[i + 1] = buff;
                f = 1;
            }
        }
        if (f == 0)
            break;
    }
    while (queue.size() > 2)
    {
        HaffmanNode* node = new HaffmanNode();
        node->val = -1;
        node->right = queue.back();
        queue.pop_back();
        node->left = queue.back();
        queue.pop_back();
        node->freq = node->left->freq + node->right->freq;
        auto i = queue.begin();
        while (((*i)->freq > node->freq) && i != queue.end() - 1)
            i++;
        queue.insert(i, node);
    }
    if (queue.size() == 2)
    {
        root = new HaffmanNode();
        root->val = -1;
        root->right = queue.back();
        queue.pop_back();
        root->left = queue.back();
        queue.pop_back();
        root->freq = root->left->freq + root->right->freq;
    }
    else if (queue.size() == 1)
    {
        root = new HaffmanNode();
        root = queue.back();
        queue.pop_back();
    }
	hist;
	CreateEncodingTable();
}

void HaffmanTree::CreateEncodingTable(HaffmanNode* node, std::string code, std::string direct)
{
	if (node->val != -1) {
		encodingTable[node->val] = code + direct;
	}
	else {
		CreateEncodingTable(node->left, code + direct, std::string("0"));
		CreateEncodingTable(node->right, code + direct, std::string("1"));
	}
}

std::string HaffmanTree::getSimbolCode(int val)
{
	return encodingTable[val];
}

std::vector<std::string> HaffmanTree::getImgEncoded(Mat src)
{
	std::vector<std::string> res;
	std::stringstream ss;
	ss << src.rows;
	ss.str();
	res.push_back(ss.str());
	std::stringstream ss2;
	ss2 << src.cols;
	ss2.str();
	res.push_back(ss2.str());
	for (int y = 0; y < src.rows; y++)
		for (int x = 0; x < src.cols; x++)
		{
			Vec3b bgr = src.at<Vec3b>(y, x);
			res.push_back(getSimbolCode(bgr[0]));
		}
	return res;
}

Mat HaffmanTree::CreateImg(std::vector<std::string> vec)
{
	std::string a = vec[0];
	int r = atoi(a.c_str());
	a = vec[1];
	int c = atoi(a.c_str());
	Mat res(r, c, 16);
	for (int i = 2; i < vec.size(); ++i)
	{
		int val = getColor(vec[i]);
		int y = (i - 2) / c;
		int x = (i - 2) % c;
		res.at<Vec3b>(y, x)[0] = val;
		res.at<Vec3b>(y, x)[1] = val;
		res.at<Vec3b>(y, x)[2] = val;
	}
	return res;
}

int HaffmanTree::getColor(std::string s)
{
	HaffmanNode* buff = root;
	for (int i = 1; i < s.size(); i++)
	{
		const char ch = s.c_str()[i];
		if (ch == '0')
			buff = buff->left;
		else if (ch == '1')
			buff = buff->right;
	}
	return buff->val;
}

void HaffmanTree::CreateEncodingTable()
{
	for (int i = 0; i < 256; i++) encodingTable.push_back("");
	CreateEncodingTable(root, "", "0");
}

HaffmanTree::~HaffmanTree()
{
    std::stack<HaffmanNode*> st;
    st.push(root);
    while (!st.empty())
    {
        HaffmanNode* curr = st.top();
        st.pop();
        if (curr)
        {
            if (curr->left)
                st.push(curr->left);
            if(curr->right)
                st.push(curr->right);
        }
        delete curr;
    }
}


Mat PhotoshopGray(Mat src)
{
    Mat res;
    src.copyTo(res);
    for (int y = 0; y < src.rows; y++)
        for (int x = 0; x < src.cols; x++)
        {
            Vec3b bgr = src.at<Vec3b>(y, x);
            float gray = bgr[0] * 0.11f + bgr[1] * 0.59f + bgr[2] * 0.3f;
            res.at<Vec3b>(y, x)[0] = gray;
            res.at<Vec3b>(y, x)[1] = gray;
            res.at<Vec3b>(y, x)[2] = gray;
        }
    return res;
}

int main(int argc, char** argv)
{
    srand(time(NULL));
    Mat img = imread("C:\\Users\\dimen\\Pictures\\sarcasm.jpg");
    Mat grey = PhotoshopGray(img);
    imshow("Original", grey);
    HaffmanTree a(grey);
	std::vector<std::string> code = a.getImgEncoded(grey);
	for(auto i = code.begin(); i < code.end(); i++)
		std::cout << *i << " ";
	Mat test = a.CreateImg(code);
	imshow("Encoded", test);
    waitKey();
    return 0;
}