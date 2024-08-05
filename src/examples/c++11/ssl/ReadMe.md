# How to generate ca.pem and server.pem key-pair files

## Install openssl
```bash
sudo apt update && sudo apt install openssl -y
```

## Generate the Root CA Certificate
1. Generate the Root CA Private Key:
```bash
openssl genpkey -algorithm RSA -out ca.key -aes256
```

2. Create the Root CA Certificate:
```bash
openssl req -x509 -new -nodes -key ca.key -sha256 -days 3650 -out ca.pem
```

## Generate the Server Certificate
1. Generate the Server Private Key:
```bash
openssl genpkey -algorithm RSA -out server.key -aes256
```

2. Create a Certificate Signing Request (CSR):
```bash
openssl req -new -key server.key -out server.csr
```

3. Sign the Server Certificate with the Root CA:
```bash
openssl x509 -req -in server.csr -CA ca.pem -CAkey ca.key -CAcreateserial -out server.pem -days 3650 -sha256 -extfile server_cert_ext.cnf -extensions v3_req
```

## Verify the Certificates
1. Verify the Root CA Certificate:
```bash
openssl x509 -in ca.pem -text -noout
```

1. Verify the Server Certificate::
```bash
openssl x509 -in server.pem -text -noout
```