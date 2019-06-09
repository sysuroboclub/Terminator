from aip import AipSpeech
# 读取文件
def get_file_content(filePath):
    with open(filePath, 'rb') as fp:
        return fp.read()
if __name__ == '__main__':
	
	""" 你的 APPID AK SK """
	APP_ID = '你的 App ID'
	API_KEY = '你的 Api Key'
	SECRET_KEY = '你的 Secret Key'

	client = AipSpeech(APP_ID, API_KEY, SECRET_KEY)

	# 识别本地文件
	#原始 PCM 的录音参数必须符合 8k/16k 采样率、16bit 位深、单声道，
	#支持的格式有：pcm（不压缩）、wav（不压缩，pcm编码）、amr（压缩格式）。
	#dev_pid不填写lan参数生效，都不填写，默认1537（普通话 输入法模型）
	msg = client.asr(get_file_content('audio.wav'), 'wav', 16000, {
	    'dev_pid': 1536,
	})
	print(msg['result'][0])