# Webserv

## 🗣️ Scope of the project

The goal of this project is to create a Web Server using the HTTP protocol.

It's main features include :

-> treatment of GET, POST and DELETE http requests <br>
-> kernel queue for requests <br>
-> HTTP compliant response status codes (404, 503, etc...) <br>
-> compatibility with Google Chrome and other mainstream Web Browsers <br>
-> CGI for dymanic websites written in PHP. <br>
-> NGINX-like configuration file that handles features such as multiple servers and maximum body size <br>
-> upload of files <br>

Please refers to the subject PDF for more information.

## 💿 Compilation and Usage

Compile everything with the ``make`` command - compatible with Linux & MacOS.  

You can run the program by using the command ``./webserv <path_to_your_config_file>`` and see that it's working by going to ``http://CONFIG_HOST:CONFIG_PORT/`` in your browser.

Note that the ```www``` folder contains some test websites you can use.

## 🗑️ Deletion

Use the Makefile rules. Write ```make fclean``` to delete the executable and the object files and ```make clean``` to remove only the object files.

## 💻 Testing

One requirement for this project is that Webserv needs to handle with success many requests in the same time. We tested it with ```siege```, and ended up with a rate of avaliability superior to ```99,5%``` when flooding the server with GET requests. 

## ✅ Validation

This project has been validated with a mark of 100/100.
