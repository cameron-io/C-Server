#!make
include ../.env

CURL_FORMAT_CFG = curl-format.txt

.PHONY: time_request
time_request: $(CURL_FORMAT_CFG)
	curl -w "@$(CURL_FORMAT_CFG)" -o /dev/null -s http://localhost:$(PORT)/
