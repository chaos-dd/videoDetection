package videoDetectionJNI;

import java.io.BufferedReader;
import java.io.IOException;
//import java.io.InputStream;
import java.io.InputStreamReader;
//import java.io.OutputStreamWriter;

public class VideoDetecion {
	static {

		// //debug
		System.load("D:/CodeCollection/videoDetection-v5/x64/Debug/dll_video_detect.dll");
		System.load("D:/CodeCollection/videoDetection-v5/x64/Debug/vd_jni_impl.dll");

		// //release
		// System.load("D:/CodeCollection/videoDetection-v5/x64/Release/dll_video_detect.dll");
		// System.load("D:/CodeCollection/videoDetection-v5/x64/Release/vd_jni_impl.dll");

//	if (System.getProperty("sun.arch.data.model").equals("32")) {
//		// 32-bit JVM
//		System.loadLibrary("/x86/dll_video_detect");
//		System.loadLibrary("/x86/vd_jni_impl");
//	} else {
//	
//		//System.loadLibrary("/lib/x64/opencv_ffmpeg249_64");
//		//System.loadLibrary("/lib/x64/opencv_core249");
//		//System.loadLibrary("/lib/x64/opencv_imgproc249");
//		//System.loadLibrary("/lib/x64/opencv_features2d249");			
//		//System.loadLibrary("/lib/x64/opencv_highgui249");
//		
//		System.loadLibrary("/lib/x64/dll_video_detect");
	//	System.loadLibrary("/lib/x64/vd_jni_impl");
//		}
	}

	/*
	 * 使用本类中的任何方法前，必须调用initModule，初始化
	 * 
	 * @configFilePath：配置文件路径
	 * 
	 * @return：成功0，失败-1
	 */
	public static native int initModule(String configFilePath);

	/*
	 * 获取帧
	 * 
	 * @ videoName 视频文件名（带路径）
	 * 
	 * @ second 要获取帧的时间点，
	 * 
	 * @frameName 帧的存储名字，以例如 c:/test/news_2.jpg,必须以 .jpg 结尾
	 * 
	 * @return 成功返回0，失败返回-1
	 */
	public static native int getFrame(String videoName, int second,
			String frameName);

	/*
	 * 预处理单个视频，//处理后的数据放在视频文件上级目录的_data文件夹内
	 * 
	 * @videoName 视频路径
	 * @return 成功返回0，失败返回-1
	 * */
	public static native int processSingleVideo(String videoName);
	
	/*
	 * @videoPath 待检测视频文件名（带路径）
	 * @targetVideoNames: 库里面的视频，待检索的视频。
	 * 
	 * 
	 * @return string 包含json结果的字符串，如果失败，字符串为空
	 */

	public static native String detectSingleVideo(String videoName);	
	public static native String detectSingleVideo(String videoName,String[] targetVideoNames);
	

	/*
	 * @videoPaths:多个视频文件名（带路径）
	 * @targetVideoNames:库里面的视频，待检索的视频。
	 * 
	 * @return String[] 包含结果的json字符串数组，个数与输入videpaths个数相同，如果某一个视频检测失败，对应的string为空
	 */
	public static native String[] detectMultipleVideos(String[] videoNames);
	public static native String[] detectMultipleVideos(String[] videoNames,String[] targetVideoNames);
	
	/*
	 * @videoName:输出视频名
	 * @mode: 1 中文，2英文
	 * 
	 * return String 音频转换的文本：
	 * 
	 	Opening input media file ...
	  	0: [00:01.24] 9340 二十五号上午，被告人薄熙来受贿贪污滥用职权一案庭审进入第四天，全案法庭调查结束。
		1: [00:10.68] 6890 开庭后法庭继续围绕不起来？滥用职权罪的犯罪事实进行取证取证。
		2: [00:17.57] 7830 公诉人被告人及其辩护人就证人王立群二十四号下午出庭所做证言进行指正。
		3: [00:25.46] 5940 薄熙来及其辩护人提出，王立军证言的客观性真实性存在疑问。		
		
	 * */
	public static String speechToText(String videoName,int mode) throws IOException{
		
		if(mode!=1 && mode !=2){
			return "";
		}		
		
		String text=new String();
		Runtime r=Runtime.getRuntime();
		try {			
			
			String cmd=" cmd /c ";
			//cmd[2]=" D:\\CodeCollection\\testCpp\\Debug\\testCpp.exe ";
			String program=" vdo2txt\\vdo2txt.exe ";
			
			String argumentString=String.format(" %s.lrc  %d sr 50 ", new Object[]{videoName,new Integer(mode)});
			
			String commandString=cmd+program+videoName+argumentString;
			
			Process proc=r.exec(commandString);

			BufferedReader br = new BufferedReader(new InputStreamReader(proc.getInputStream()));  

			String line;
			
	        while((line=br.readLine()) != null){  
	            //System.out.println(line);  
	        	text +='\n'+line;
	        }  		
			
		} catch (StringIndexOutOfBoundsException e) {
			// TODO: handle exception
			System.out.println("Error!");
			return "";
			
		} 
		return text;
	}

}
