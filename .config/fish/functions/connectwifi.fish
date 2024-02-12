function connectwifi --wraps='sudo nmcli --ask dev wifi connect' --description 'alias connectwifi=sudo nmcli --ask dev wifi connect'
  sudo nmcli --ask dev wifi connect $argv
        
end
