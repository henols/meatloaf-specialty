#/bin/bash
gh act -s GITHUB_TOKEN="$(gh auth token)" \
        -s FTP_USER=meatloaf \
        -s 'FTP_PASSWORD=oL3G78zR9xNt&v' \
        --var FTP_SERVER=aceone.se \
        --var FTP_PORT=4021 \
        --var FTP_PATH=/Multimedia/ml/ \
        --artifact-server-path /home/henrik/tmp/artifacts/ \
        # --verbose

