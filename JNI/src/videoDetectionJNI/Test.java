package videoDetectionJNI;

import java.io.IOException;

import videoDetectionJNI.VideoDetecion;

public class Test {
	static{
		
		int ret = VideoDetecion.initModule("D:\\CodeCollection\\videoDetection-v5\\JNI\\det_config.xml");
		System.out.println(ret);
	}
	public int test_getFrame(){
		System.out.println("test_getFrame\n");
		
		String videoName="D:\\videos\\detect1\\2.mp4";
		int second =10;
		String frameName="D:\\videos\\detect1\\0-10.jpg";
		
		if( VideoDetecion.getFrame(videoName, second, frameName)==-1){
			System.out.println("failed\n");
			return -1;
		}
		else {
			System.out.println("success\n");
			return 0;
		}	
	}
	
	
	public int test_detectSingleVideo2Video(){
		String videoName1="D:\\videos\\send\\det_0.mp4";
		String[] videoName2={"D:\\videos\\send\\tar_2.mp4"};
		
		String jsonResult;
		jsonResult=VideoDetecion.detectSingleVideo(videoName1,videoName2);
		
		
		if(jsonResult.length()==0){
			System.out.println("failed\n");
			return -1;
		}
		else {
			System.out.println("success\n");			
			System.out.println(jsonResult);
			return 0;
		}
	}
	
	public int test_detectSingleVideo(){
		System.out.println("test_detectSingleVideo");
		String videoName="D:\\videos\\detect1\\1.wmv";
		
		String jsonResult;
		jsonResult=VideoDetecion.detectSingleVideo(videoName);
		
		
		if(jsonResult.length()==0){
			System.out.println("failed\n");
			return -1;
		}
		else {
			System.out.println("success\n");			
			System.out.println(jsonResult);
			return 0;
		}
	}
	public int test_detectSingleVideo2(){
		System.out.println("test_detectSingleVideo2");
		String videoName="D:\\videos\\detect1\\1.wmv";
		
		String jsonResult;
		jsonResult=VideoDetecion.detectSingleVideo(videoName,new String[]{"D:\\videos\\target1\\0.mp4","D:\\videos\\target1\\4.mp4"});
		
		
		if(jsonResult.length()==0){
			System.out.println("failed\n");
			return -1;
		}
		else {
			System.out.println("success\n");			
			System.out.println(jsonResult);
			return 0;
		}
	}
	public int test_detectMultipleVideos(){
		System.out.println("test_detectMultipleVideos");
		String[] videoNames=new String[2];
		videoNames[0]="D:\\videos\\detect1\\1.wmv";
		videoNames[1]="D:\\videos\\detect1\\2.mp4";
		
		String[] jsonResults;
		jsonResults=VideoDetecion.detectMultipleVideos(videoNames);
		if(jsonResults.length==0){
			System.out.println("failed\n");
			return -1;
		}
		else {
			System.out.println("success\n");
			
			for(int i =0 ; i != jsonResults.length;++i){
				System.out.println(jsonResults[i]);
			}
			
			return 0;
		}	
		
	}
	public int test_speechToText(){
		String videoName="D:\\demos\\ÒôÆµ×ªÎÄ×Ö\\1.mp4";
		try {
			String textString=VideoDetecion.speechToText(videoName,1);
			System.out.println(textString);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
		return 0;		
	}

	public static void main(String[] args){
		Test testInstnce = new Test();
		
		//testInstnce.test_getFrame();
		//testInstnce.test_detectSingleVideo();
		//testInstnce.test_detectMultipleVideos();
		
		//testInstnce.test_speechToText();
		
		//testInstnce.test_detectSingleVideo();
		//testInstnce.test_detectSingleVideo2();
		
		testInstnce.test_detectSingleVideo2Video();
		
		System.out.println("done!");
		
	}
}
