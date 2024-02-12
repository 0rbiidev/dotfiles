function listwifi --wraps='nmcli dev wifi list' --description 'alias listwifi=nmcli dev wifi list'
  nmcli dev wifi list $argv
        
end
