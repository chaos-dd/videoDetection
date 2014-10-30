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
	 * ʹ�ñ����е��κη���ǰ���������initModule����ʼ��
	 * 
	 * @configFilePath�������ļ�·��
	 * 
	 * @return���ɹ�0��ʧ��-1
	 */
	public static native int initModule(String configFilePath);

	/*
	 * ��ȡ֡
	 * 
	 * @ videoName ��Ƶ�ļ�������·����
	 * 
	 * @ second Ҫ��ȡ֡��ʱ��㣬
	 * 
	 * @frameName ֡�Ĵ洢���֣������� c:/test/news_2.jpg,������ .jpg ��β
	 * 
	 * @return �ɹ�����0��ʧ�ܷ���-1
	 */
	public static native int getFrame(String videoName, int second,
			String frameName);

	/*
	 * Ԥ��������Ƶ��//���������ݷ�����Ƶ�ļ��ϼ�Ŀ¼��_data�ļ�����
	 * 
	 * @videoName ��Ƶ·��
	 * @return �ɹ�����0��ʧ�ܷ���-1
	 * */
	public static native int processSingleVideo(String videoName);
	
	/*
	 * @videoPath �������Ƶ�ļ�������·����
	 * @targetVideoNames: ���������Ƶ������������Ƶ��
	 * 
	 * 
	 * @return string ����json������ַ��������ʧ�ܣ��ַ���Ϊ��
	 */

	public static native String detectSingleVideo(String videoName);	
	public static native String detectSingleVideo(String videoName,String[] targetVideoNames);
	

	/*
	 * @videoPaths:�����Ƶ�ļ�������·����
	 * @targetVideoNames:���������Ƶ������������Ƶ��
	 * 
	 * @return String[] ���������json�ַ������飬����������videpaths������ͬ�����ĳһ����Ƶ���ʧ�ܣ���Ӧ��stringΪ��
	 */
	public static native String[] detectMultipleVideos(String[] videoNames);
	public static native String[] detectMultipleVideos(String[] videoNames,String[] targetVideoNames);
	
	/*
	 * @videoName:�����Ƶ��
	 * @mode: 1 ���ģ�2Ӣ��
	 * 
	 * return String ��Ƶת�����ı���
	 * 
	 	Opening input media file ...
	  	0: [00:01.24] 9340 ��ʮ������磬�����˱������ܻ�̰������ְȨһ��ͥ���������죬ȫ����ͥ���������
		1: [00:10.68] 6890 ��ͥ��ͥ����Χ�Ʋ�����������ְȨ��ķ�����ʵ����ȡ֤ȡ֤��
		2: [00:17.57] 7830 �����˱����˼���绤�˾�֤������Ⱥ��ʮ�ĺ������ͥ����֤�Խ���ָ����
		3: [00:25.46] 5940 ����������绤�������������֤�ԵĿ͹�����ʵ�Դ������ʡ�		
		
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
