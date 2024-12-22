// 초음파 센서
const int UP_TRIG_PIN = 2; // UP 초음파 측정기
const int UP_ECHO_PIN = 3; 
const int DOWN_TRIG_PIN = 4;  // DOWN 초음파 측정기
const int DOWN_ECHO_PIN = 5;
const int LED_PIN = 13;
const int MAIN_DISTANCE = 20; // 거리의 임계값

// 모터 & IR센서
#define SPEED 150   // 이동 속도 상수
const int IR_leftSensor = 10;   // 왼쪽 적외선 센서 핀 번호
const int IR_rightSensor = 11;   // 오른쪽 적외선 센서 핀 번호
const int A1_motor = 9;   // 왼쪽 모터 핀 번호 (전진)
const int A2_motor = 8;   // 왼쪽 모터 핀 번호 (후진)
const int B1_motor = 7;   // 오른쪽 모터 핀 번호 (전진)
const int B2_motor = 6;   // 오른쪽 모터 핀 번호 (후진)


// 아이가 초음파에 감지되면 로봇을 멈출 떄 까지의 초음파 상태 변수
bool sonar_result = false;


// 0 : 정지 (전진 에정)
// 1 : 앞으로 전진중
// 2 : 정지 (후진 예정)
// 3 : 뒤로 후진중
enum STATES {F_STOP, F_MOVE, B_STOP, B_MOVE} // 열거형으로 상태 정의
int state = 0;

// 초음파 측정 카운터
int sonar_count = 0;

// 왼쪽 모터 회전
void left_front(int i) 
{
  digitalWrite(A2_motor, LOW);   // 후진 핀에 LOW 신호 출력
  analogWrite(A1_motor, i);   // 전진 핀에 PWM 신호 출력 150
}

// 왼쪽 모터 역회전
void left_back(int i)
{
  digitalWrite(A2_motor, HIGH);   // 후진 핀에 HIGH 신호 출력
  analogWrite(A1_motor, 255 - i);   // 전진 핀에 (255-i)의 PWM 신호 출력 105
}

// 왼쪽 모터 정지
void left_stop()
{
  left_front(0);   // 왼쪽 모터 PWM 신호를 0으로 설정
}

// 오른쪽 모터 회전
void right_front(int i)
{
  digitalWrite(B2_motor, HIGH);   // 후진 핀에 HIGH 신호 출력
  analogWrite(B1_motor, 255 - i);   // 전진 핀에 (255-i)의 PWM 신호 출력 105
}

// 오른쪽 모터 역회전
void right_back(int i)
{
  digitalWrite(B2_motor, LOW);   // 후진 핀에 LOW 신호 출력
  analogWrite(B1_motor, i);   // 전진 핀에 PWM 신호 출력   150
}

// 오른쪽 모터 정지
void right_stop()
{
  right_front(0);   // 오른쪽 모터 PWM 신호를 0으로 설정
}

void move()
{
  left_front(SPEED);
  right_front(SPEED);
}

void stop()
{
  left_front(0);
  right_front(0);
}

void back()
{
  left_back(SPEED);
  right_back(SPEED);
}

void setup() 
{
  // 초음파 센서
  pinMode(UP_TRIG_PIN, OUTPUT);
  pinMode(UP_ECHO_PIN, INPUT);
  pinMode(DOWN_TRIG_PIN, OUTPUT);
  pinMode(DOWN_ECHO_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  
  // IR센서 & 모
  pinMode(IR_leftSensor, INPUT);   // 왼쪽 적외선 센서를 입력 모드로 설정
  pinMode(IR_rightSensor, INPUT);   // 오른쪽 적외선 센서를 입력 모드로 설정
  pinMode(A2_motor, OUTPUT);   // 왼쪽 모터의 후진 핀을 출력 모드로 설정
  pinMode(B2_motor, OUTPUT);   // 오른쪽 모터의 후진 핀을 출력 모드로 설정

  stop();
  
  Serial.begin(9600);
}

void loop() 
{

// 상태결정용 소나 측정
  long duration, distance_UP, distance_DOWN;
  digitalWrite(UP_TRIG_PIN, LOW);  // UP 초음파 센서 초기화
  delayMicroseconds(2);
  digitalWrite(UP_TRIG_PIN, HIGH);  // UP 초음파 센서에 초음파 보냄
  delayMicroseconds(10);
  digitalWrite(UP_TRIG_PIN, LOW);
  duration = pulseIn(UP_ECHO_PIN, HIGH);  // UP 초음파 센서로부터 반사된 초음파 신호를 받아 거리 계산
  distance_UP = duration / 58;

  digitalWrite(DOWN_TRIG_PIN, LOW);  // DOWN 초음파 센서 초기화
  delayMicroseconds(2);
  digitalWrite(DOWN_TRIG_PIN, HIGH);  // DOWN 초음파 센서에 초음파 보냄
  delayMicroseconds(10);
  digitalWrite(DOWN_TRIG_PIN, LOW);
  duration = pulseIn(DOWN_ECHO_PIN, HIGH);  // DOWN 초음파 센서로부터 반사된 초음파 신호를 받아 거리 계산
  distance_DOWN = duration / 58;

  // UP센서와 DOWN센서가 사람을 인식하는 거리를 실시간으로 출력
  Serial.print("Distance UP: ");
  Serial.print(distance_UP);
  Serial.println(" cm");
  Serial.print("Distance DOWN: ");
  Serial.print(distance_DOWN);
  Serial.println(" cm");

  // IR 센서----------------------------------------------------------------------------------
  int front_status = digitalRead(IR_leftSensor);   // 왼쪽 적외선 센서의 상태를 읽어옴
  int back_status = digitalRead(IR_rightSensor);   // 오른쪽 적외선 센서의 상태를 읽어옴
  // -----------------------------------------------------------------------------------------


  // 초음파 센서
  if (distance_UP > MAIN_DISTANCE && distance_DOWN <= MAIN_DISTANCE)  // 아이가 감지됐을 때
  {
    sonar_count = sonar_count + 1;
    if(sonar_count == 5) { sonar_result = true; }
    Serial.print("count : ");
    Serial.println(sonar_count);
  }
  else 
  {
    sonar_result = false;
  }
  //-------------------------------------------

  // 로봇의 상태
  if (state == F_STOP){ // 출발점 정지
    if (sonar_result == true) {
      digitalWrite(LED_PIN, HIGH);
      state = F_MOVE; move();
    }
    else {
      digitalWrite(LED_PIN, LOW);
      state = F_STOP;
    }
  }
  else if (state == F_MOVE){ // 전진중
    if (front_status == 1 && back_status == 0){
      digitalWrite(LED_PIN, LOW);
      state = B_STOP; stop();
    }
    else{
      digitalWrite(LED_PIN, HIGH);
      state = F_MOVE; move();
      sonar_count = 0;
    }
  }
  else if (state == B_STOP){ // 도착점 정지
    if (sonar_result == true){
      digitalWrite(LED_PIN, HIGH);
      state = B_MOVE; back();
    }
    else{
      digitalWrite(LED_PIN, LOW);
      state = B_STOP; stop();
    }
  }
  else if (state == B_MOVE){ // 후진중
    if (front_status == 0 && back_status == 1){
      digitalWrite(LED_PIN, LOW);
      state = F_STOP; stop();
    }
    else{
      digitalWrite(LED_PIN, HIGH);
      state = B_MOVE; back();
      sonar_count = 0;
    }
  }
  delay(50);
}
