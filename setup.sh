#!/bin/bash

set -e

echo "Creating directories..."

# Diretórios principais
mkdir -p output/{connections_log,db,server_log}

echo "Creating files..."

# Logs
touch output/connections_log/client_connections.log
touch output/connections_log/client_messages.log
touch output/server_log/server.log

# Banco de dados (arquivo texto)
touch output/db/db.txt

echo "Setup executed!"