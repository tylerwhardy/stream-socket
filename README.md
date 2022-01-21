# stream-socket
Stream socket in C

Question 1: Do the pairs of programs interfere with each other? 
No. The client/server pair utilize TCP (which can be validated by the socktype attribute) and the listener/talker pair utilize UDP. Any given request is unique to the source/destination IP, source/destination port, and the protocol in use. If one request utilizes UDP and another request utilizes TCP, the requests still maintain independence despite using the same port number. 
Question 2: Print a copy of the information the server receives. Identify the START_LINE and MESSAGE_HEADER that your new server receives from the browser. See the description of the request and response messages in section 9.1.2, and identify these fields in your print out.
 

 

START_LINE: GET / HTTP/1.1
MESSAGE_HEADER information: User-Agent, Accept, Accept-encoding, Host, Connection Type
Project Discussion:
This is the implementation of a custom TCP protocol which utilizes a 6 digit code to request what to have for dinner. 
The server is started by executing the mp1server executable. This sets port 3490 to listen for incoming TCP requests. Upon receipt of any repeating 6 digit integer code between 1 and 5 via port 3490, the server will return a food item. 
Input sanitization is achieved first by checking the argument passed for the correct number of segments. In the event that an incorrect number of arguments are passed, the program exits with an error. If the code input is any value other than 6 in length, the program exits with an error. If the code contains non-numeric characters then the program exits with an error. 
Both the client and server are set to a maximum bytesize of 10,000. This ensures that sufficient memory exists to handle any expected requests. The client is protected by setting the array location that matches the sent size to the null set. This should protect against buffer overflows. 
In terms of project history, the source code was adapted from client.c, listener.c, server.c, and talker.c. Source code was obtained from Brian “Beej” Hall’s Network Programming Guide. Upon first viewing the code, I decided to focus on refamiliarizing myself with C by first installing error proofing into the default client.c source code. I infrequently use Linux so I had to perform some self-study on syntax and various use cases. I primarily use Python at work so transitioning to a low-level language required some time for recollecting my undergraduate experience.
After establishing standard error protection, I created the code that would transmit a test packet to the server application. I then added proper receipt code to the server. From there, I created a switch to assign the correct return message to the server’s memory for transmission. I then added code to transmit that message. 
In terms of project difficulty, my primary problem was in incorrectly using sockfd in a location where new_fd should have been used. This caused my socket connection to fail which took an extremely long time to figure out. Another area of concern was correctly processing strings and integers. This required a bit of research to get working correctly. Most of my problems were solved by the addition of breakpoint messages to help debug the code. 
