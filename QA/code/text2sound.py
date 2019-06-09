from aip import AipSpeech
# 读取文件
def get_file_content(filePath):
    with open(filePath, 'rb') as fp:
        return fp.read()


if __name__ == '__main__':
	APP_ID = '15669307'
	API_KEY = '5e8pu66frju7dF0k3owBy7m5'
	SECRET_KEY = 'IVDIZXguZmkKYwGXkVC6G65edc10jZtN'

	client = AipSpeech(APP_ID, API_KEY, SECRET_KEY)

	text = '你好啊小老弟'
	result  = client.synthesis(text, 'zh', 1, {
	    'vol': 5, 'per': 0,
	})

	# 识别正确返回语音二进制 错误则返回dict 参照下面错误码
	if not isinstance(result, dict):
	    with open('auido.mp3', 'wb') as f:
	        f.write(result)