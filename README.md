# Scope of the project

The goal of this project is to create a Web Server using the HTTPS protocol (see the subject PDF file for more information about the features).

## Testing
In the test folder, a stresser.sh bash script is available. You can use it this way:  
> **./stresser.sh URL number_of_requests [number_of_simultaneous_connections]**

It will send requests to your web server as fast as possible.
> 1st argument is the URL (http://localhost:8080/)  

> 2nd argument is the amount of requests that will be sent 

> 3rd argument is the amount of simultaneous connections we will use to send theses requests 

You'll see a resume with the amount of successfull requests, the amount of unsuccessfull requests (there should be none but we never know), and the time it took.
