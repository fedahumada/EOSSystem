certutil.exe -addstore root .\AmazonRootCA1.cer
certutil.exe -addstore root .\AmazonRootCA2.cer
certutil.exe -addstore root .\AmazonRootCA3.cer
certutil.exe -addstore root .\AmazonRootCA4.cer
certutil.exe -addstore root .\SFSRootCAG2.cer
.\EOSSystemServer.exe -log -AUTH_TYPE=deviceid -AUTH_LOGIN=test -AUTH_TOKEN=test