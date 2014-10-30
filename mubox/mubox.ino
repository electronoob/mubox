/*

    Input / Output for microcontroller projects. CMD like.

*/

#define HOST_NAME "µbox"
#define HOST_ID 1
#define HOST_BANNER \
		" µß°× terminal \r\n"\
		"\r\n"\
		"Press enter to begin."

#define LOGIN_PROMPT "Login: "
#define PASSWORD_PROMPT "Password: "
#define mubox_username "root"
#define mubox_password "toor"

boolean have_tty = 0;
boolean have_auth = 0;

int ticks = 0;

void setup() {
  Serial.begin(115200);
  /* disable local echo - turn on send-receive mode */
  vt100_ESC("e[12h");
  show_tty_welcome();
}

void loop() {
  // checks if connected yet - if not, show mubox header
  show_banner(); 
  // next we make sure authorized
  if (show_authorize()) {
    // repl loop
    if ((have_tty) && (have_auth)) {
      vt100_color_dim_white_normal ();
      repl_loop();
      ticks = 0;
      have_tty = 0;
      have_auth = 0;
      vt100_color_dim_green_normal ();
      echo ("\r\nLogged out.\r\n");
      delay(2000);
      show_tty_welcome();
      return;
    }
  } else {
    delay(3000);
    ticks = 0;
    have_tty = 0;
    have_auth = 0;
    show_tty_welcome();
    return;
  }
  delay(100);
  ticks++;
}

void repl_loop(){
  for(;;) {
    echo ("\r\n(DDS) µ ");
    
    return;
  }
}

int show_authorize() {
if (get_key(0x0D))
  if (!have_tty) {
    have_tty = 1;
    // need to show login
    vt100_ESC("e[1K");
    int failures=0;
    while ( !get_auth_prompt() ) {
      failures++;
      if(failures == 3) { have_tty = 0; return 0; }
    }
    have_tty = 1;
    return 1;
  }
}
int get_key(char key) {
	if(Serial.available()>0) {
    	while(Serial.available() >0 ) {
    		char ch = Serial.read();
			if (ch == key) return 1;
		}
	}
	return 0;
}
void show_banner() {
  if(ticks>100) {
    if (!have_tty) { 
      /* display connection banner */
      show_tty_welcome();
    }
  ticks=0;
  }
  
}

void echo(char buffer[40]) {
  Serial.print(buffer);
}
int get_auth_prompt() {
  //begin
  //vt100_clear();
  boolean loop = 0;

    echo("\r\n");
    vt100_ESC("e[1K");
    echo(LOGIN_PROMPT);
    char username[128] = {0};
    char *source = get_line(0);
    memcpy(username, source, 128);
    
    echo("\r\n");
    vt100_ESC("e[1K");
    echo(PASSWORD_PROMPT);
    char password[128] = {0};
    source[0] = 0;
    source = get_line(1);
    memcpy(password, source, 128);
    echo("\r\n");
    vt100_ESC("e[1K");
    
    if ((memcmp(username, mubox_username, 5) == 0) && (memcmp(password, mubox_password, 5) == 0)) {
      vt100_color_dim_white_normal ();
      have_auth=1;
      return 1;
    }
    delay(1000);
    return 0;
}
void vt100_clear() {
  vt100_ESC("e[2J");
}
void vt100_ESC(char *buffer) {
  buffer[0] = 0x1B;
  echo(buffer); 
}
void vt100_color_bright_red_inverted () {
  vt100_ESC("e[7;1;31m");
}
void vt100_color_dim_white_normal () {
  vt100_ESC("e[0;2;37m");
}
void vt100_color_bright_white_inverted () {
  vt100_ESC("e[7;1;37m");
}
void vt100_color_dim_green_normal () {
  vt100_ESC("e[0;2;32m");
}
void vt100_home() {  
  char buffer[] = "e[H";
  buffer[0] = 0x1B;
  echo(buffer);
}
void show_tty_welcome () {

  // enable scrolling? -- key to getting the page to start at top :D
  //e[r
  char scrolling[] = "e[r";
  scrolling[0] = 0x1B;
  echo(scrolling);
  vt100_clear();
  vt100_color_bright_red_inverted ();
  echo(HOST_BANNER); 
  vt100_color_bright_white_inverted ();
  
}

// blocking
char *get_line(boolean secret) {
  //echo("begin get line");
  char buffer[128] = {0};
  char ch = 0;
  int i = 0;
  while (ch != 0x0D) {
    if(Serial.available() >0){
      ch = Serial.read();
      if(ch!=0x0D) {
        /* let's try to handle delete at the very leaste */
        if (ch != 0x7F) {
          buffer[i++] = ch;
        } else {
          i--;
          buffer[i] = 0x00;
          if(!secret) {
            vt100_ESC("e[D");
            vt100_ESC("e[K");
          }
          continue;
        }
        if(!secret) {echo(&buffer[i-1]);}
      }
    }
  }
  return buffer;
}