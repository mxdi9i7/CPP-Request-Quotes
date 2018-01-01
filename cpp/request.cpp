#include "request.h"
//request.h contains the http socket that's need for the program to run http requests, I got the source from a public repo, not directly related to the logic of my program
using namespace std;
int main()
{
  const char addr[] = "45.55.64.88";
   //this is a nodejs server's IP I deployed dedicated to serve my Cpp program, the code for my nodejs server can be found in directory: ./server 
  char choice;
  bool run = true;
  while(run)
  //determine whether to continue the program
  {
    http http(addr, 4999); //the port on which I deployed the server.
    char symbols[20]; //most stock symbols only have 4-5 characters, but i made it 20 to accept potential more user inputs.
    double price;
    //http json response variables
    char *respone;
    char *success;
    char *boolval;
    char *message;
    char *quote ;
    char *quote2;
    char yn;
            
    cout << "--------------------------------------------" << endl;
    cout << "\nPlease enter the stock symbol e.g. AAPL : ";

	//another way of writing cin >>
    scanf("%s", symbols);

    quote = (char*)malloc(6+strlen(symbols));
    strncpy(quote, "/api/", 5);

	//quote2 represents the user input to query the stock quote
    quote2 = quote + 5;
    strncat(quote2, symbols, strlen(symbols));
    
	//structure the http request
    http.get(quote);
    http.header_host(addr);
    http.header_range(0, 500);
    http.header_connection(false);
    http.request();

    respone = http.read_get(stdout);
    //read http response
    if((success  = strstr(respone, "success")) != NULL) {
      if((boolval = strstr(success, "true")) != NULL) {
        if((message = strstr(boolval, "data")) != NULL) {
          int size = 0;
          int qunt = 0;
          int respi = 1;
          char *final = (char*)malloc(10);
          final = message + 6;
          while(*final != '}')
          {
            size++;
            final++;
          }
		  //using atof to convert string price to float so we can use it for calculations later
          strncpy(final, (message + 6), size);
          price = atof(final);
          cout << "The stock price is : ";
          cout << price << endl;

          cout << "Please enter the number of stocks that you wish to purchase :"; 
          cin >> qunt;

          cout << "The stocks can be bought for " << price * qunt << endl;
          cout << "Thanks\n" << endl;
          cout << "********************************************" << endl;
        }
      }

	//identify the presence of success message sent from the server
    if((boolval = strstr(success, "false")) != NULL) {
      	if((message = strstr(boolval, "message")) != NULL) {
			char *final = (char*)malloc(100);
			strncpy(final, (message + 10), 46);
			cout << final << endl;
			cout << "********************************************\n" << endl;
		}
    }
    
  }
	//check if user wants to perform another query
  	cout << "Would you like to perform other calculation?(Y/N)";
  	cin >> choice;

  	if(choice =='N'||choice =='n') {
   	 	run = false;
 	}
  	free(respone);
  	http.close();
}
  return 0;
}
